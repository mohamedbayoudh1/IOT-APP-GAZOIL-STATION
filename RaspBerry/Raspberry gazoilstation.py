import paho.mqtt.client as mqtt
import json
import datetime
import sqlite3
import mysql.connector


conn = sqlite3.connect('station.db')
cnx = mysql.connector.connect(
    host="",
    user="",
    password="",
    database=""
    )
cursor = conn.cursor()
cur = cnx.cursor()

MQTT_ADDRESS = ''
MQTT_USER = ''
MQTT_PASSWORD = ''
MQTT_TOPIC = 'home/pi/pi/data'
waterlevel=""
fire=""
def create_database():
    conn = sqlite3.connect('station.db')
    cursor = conn.cursor()
    cursor.execute('''CREATE TABLE IF NOT EXISTS tank
                      (id INTEGER PRIMARY KEY AUTOINCREMENT,
                       waterlevel INTEGER,
                       fire INTEGER,
                       date DATETIME)''')
    conn.commit()
    conn.close()


def on_connect(client, userdata, flags, rc):
    """ The callback for when the client receives a CONNACK response from the server."""
    print('Connected with result code ' + str(rc))
    client.subscribe(MQTT_TOPIC)





def on_message(client, userdata, msg):
    data = json.loads(msg.payload.decode())
    waterlevel= data["value1"]
    fire= data["value2"]
        
    
    cursor.execute("INSERT INTO tank (waterlevel,fire,date) VALUES (?, ?, ?)", (waterlevel,fire,datetime.datetime.now()))
    cursor.execute("SELECT * FROM tank")
    results = cursor.fetchall()
    for result in results:
       print(result)

    conn.commit()
    
    #query = "INSERT INTO traitev (waterlevel,fire) VALUES (%s, %s)" 
    #paras = (waterlevel,fire)
    #cur.execute(query, paras)
    #cnx.commit()
    
    """The callback for when a PUBLISH message is received from the server."""
    print("Received data from ESP8200:")
    print("Niveau de gazOIL: ", waterlevel)
    print("Etat de fire: ", fire)

def main():
    create_database()
    mqtt_client = mqtt.Client()
    mqtt_client.username_pw_set(MQTT_USER, MQTT_PASSWORD)
    mqtt_client.on_connect = on_connect
    mqtt_client.on_message = on_message

    mqtt_client.connect(MQTT_ADDRESS, 1883)
    mqtt_client.loop_forever()

if __name__ == '__main__':
    # main code goes here
    print('MQTT to InfluxDB bridge')
    main()