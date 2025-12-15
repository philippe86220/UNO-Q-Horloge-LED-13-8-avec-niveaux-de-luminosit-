from arduino.app_utils import Bridge, Logger
import time
from datetime import datetime
from zoneinfo import ZoneInfo

logger = Logger("uno-q-clock")
PARIS_TZ = ZoneInfo("Europe/Paris")

def main():
    bridge = Bridge()
    logger.info("Python main() started")

    while True:
        # Heure exacte en France, avec gestion automatique ete/hiver
        now = datetime.now(PARIS_TZ)
        h = now.hour
        m = now.minute
        s = now.second

        bridge.call("updateTime", h, m, s)
        logger.info(f"Sent time {h:02d}:{m:02d}:{s:02d}")

        time.sleep(1)

if __name__ == "__main__":
    main()

