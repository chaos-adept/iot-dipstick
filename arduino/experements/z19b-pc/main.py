from mh_z19b.sync import Sensor

sensor = Sensor(port='COM7', baudrate=9600)
print(sensor.read_metric())