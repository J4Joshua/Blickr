import asyncio
from uuid import UUID

from construct import Array, Byte, Const, Int8sl, Int16ub, Struct
from construct.core import ConstError

from bleak import BleakScanner
from bleak.backends.device import BLEDevice
from bleak.backends.scanner import AdvertisementData

from fastapi import FastAPI

ibeacon_format = Struct(
    "type_length" / Const(b"\x02\x15"),
    "uuid" / Array(16, Byte),
    "major" / Int16ub,
    "minor" / Int16ub,
    "power" / Int8sl,
)

app = FastAPI()

flag = True
choice = 1

@app.get("/")
async def root():
    return {"message": "Hello World"}


@app.get("/hello/{name}")
async def say_hello(name: str):
    return {"message": f"Hello {choice}"}


@app.get("/joshua/{name}")
async def say_hello(name: str):


    await main()








def device_found(
    device: BLEDevice, advertisement_data: AdvertisementData
):
    """Decode iBeacon."""
    #print(47 * "-")
    try:
        apple_data = advertisement_data.manufacturer_data[0x004C]
        ibeacon = ibeacon_format.parse(apple_data)
        uuid = UUID(bytes=bytes(ibeacon.uuid))
        print(f"UUID     : {uuid}")
        print(f"Major    : {ibeacon.major}")
        print(f"Minor    : {ibeacon.minor}")
        print(f"TX power : {ibeacon.power} dBm")
        print(f"RSSI     : {device.rssi} dBm")
        print(47 * "-")
        return {"message": f"Hello "}
        global choice
        choice = ibeacon.minor
    except KeyError:
        # Apple company ID (0x004c) not found
        pass
    except ConstError:
        # No iBeacon (type 0x02 and length 0x15)
        pass

async def main():
    """Scan for devices."""
    scanner = BleakScanner()
    scanner.register_detection_callback(device_found)

    while True:
        
        await scanner.start()
        await asyncio.sleep(1.0)
        await scanner.stop()

