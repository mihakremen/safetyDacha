import paho.mqtt.publish as publish

HOST = "35.198.104.194"
USER = "mihakremen"
PASS = "mihakremen"


def send_mqtt(topic, payload):
    publish.single(
        topic=topic,
        payload=payload,
        hostname=HOST,
        auth={'username': USER, 'password': PASS})


if __name__ == '__main__':
    # publish a single message
    send_mqtt("ha", "ha")
