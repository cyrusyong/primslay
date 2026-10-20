import sys
import os
import requests
import dotenv

dotenv.load_dotenv()

def load_topic():
    return os.getenv("NTFY_TOPIC")


def send(topic, message):
    if not topic:
        print("NTFY_TOPIC is not set", file=sys.stderr)
        return 1

    topic_url = f"https://ntfy.sh/{topic}"
    response = requests.post(topic_url, data=message, timeout=100)
    if response.status_code != 200:
        print(f"Failed to send message: {response.status_code}")
        return 1
    return 0


def main(argv):
    if len(argv) < 2:
        print("usage: send.py <message>", file=sys.stderr)
        sys.exit(1)

    message = " ".join(argv[1:])
    topic = load_topic()

    return send(topic, message)


if __name__ == "__main__":
    sys.exit(main(sys.argv))
