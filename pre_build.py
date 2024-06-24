import time
import os
import json
import pyduinocli

import shutil

local_config_path = "Arduino15"
local_lib_path = "libraries"

arduino = pyduinocli.Arduino("arduino-cli")
conf = arduino.config.dump()
data_dir = conf["result"]["directories"]["data"]
user_dir = os.path.join( conf["result"]["directories"]["user"] , "libraries")   


print(f"Moving arduino defines to data dir {data_dir}")

shutil.copytree(local_config_path, data_dir, dirs_exist_ok=True)

print("moving lib to arduino lib folder")

shutil.copytree(local_lib_path, user_dir, dirs_exist_ok=True)

