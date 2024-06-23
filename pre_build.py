import time
import os
import json
import pyduinocli

import shutil

local_config_path = "Arduino15"
arduino = pyduinocli.Arduino("arduino-cli")
conf = arduino.config.dump()
data_dir = conf["result"]["directories"]["data"]

print(f"Moving arduino defines to data dir {data_dir}")

shutil.copytree(local_config_path, data_dir, dirs_exist_ok=True)

