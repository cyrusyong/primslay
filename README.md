# prim display

A computer posts a text message to a secret [ntfy.sh](https://ntfy.sh) topic. An ESP32-2432S028 on any other Wi-Fi polls that topic and shows the text. The two devices never connect to each other.

The topic string is the only password. Anyone who knows it can read and post.

```
computer  --HTTPS POST-->  ntfy.sh / <topic>  --HTTPS poll-->  ESP32 display
```

## Layout

| Path | Role |
| --- | --- |
| `sender/send.py` | POST the message text to the topic. |
| `firmware/src/main.cpp` | Join Wi-Fi, sync the clock, poll, draw. |
| `firmware/src/wifi_link.cpp` | Join Wi-Fi. If ntfy is not reachable over IPv4, turn on IPv6. |
| `firmware/src/inbox.cpp` | Poll ntfy and fill an `ntfy_response`. |
| `firmware/src/display.cpp` | Wrap the message and draw it, with an `HH:MM` footer. |
| `firmware/include/display.h` | ILI9341 pins and the `tft` object. |
| `firmware/include/certs.h` | ISRG Root X1, already in the tree. |
| `firmware/include/config.h` | Host name, poll interval (5 s), NTP retry count. |

## Setup

1. Put the same topic in both private files. `secrets.h`:

   ```cpp
   #pragma once
   #define WIFI_SSID "network name"
   #define WIFI_PASSWORD "network password"
   #define NTFY_TOPIC "topic"
   ```

   `.env` in the repo root:

   ```
   NTFY_TOPIC=topic
   ```

   `secrets.h` uses the Wi-Fi the display will join.

2. Install [PlatformIO](https://platformio.org/), plug the board in over USB, and flash it. If no serial port appears, install a CH340 driver.

   ```bash
   cd firmware
   pio run -t upload
   pio device monitor
   ```

   Serial is 115200. On boot the board connects, syncs US Eastern time from NTP (up to a minute), then polls. A failed step prints on serial and the loop stops until you reset the board. A new message id is drawn centered; the footer is the message time in Eastern.

3. Sender dependencies are `requests` and `python-dotenv`. From the repo root:

   ```bash
   python3 sender/send.py "hello from the computer"
   ```
