import datetime as dt

import requests
from fastapi import FastAPI

from backend.MyModel import MyModel

app = FastAPI()

# dataset_fp = pathlib.Path()

mymodel = MyModel('backend/my_dataset.csv')


@app.get("/")
def read_root():
    return {"Hello": "World"}


def predict(
        soil_moisture: int,
        air_temperature: float,
        air_humidity: float
) -> float:
    hourofday = dt.datetime.now().hour

    probability = mymodel.predict(
        soil_moisture=soil_moisture,
        temperature=air_temperature,
        humidity=air_humidity,
        hour_ofday=hourofday
    )

    return probability[0]


pump_state = -9999


def set_blynk_pump_state(state: int) -> int:
    if state == pump_state:
        return 200

    #     https://sgp1.blynk.cloud/external/api/update?token=7ipFMtKuX0ghOOHdU0N_rBTY9vNzwq7o&V2=1
    r = requests.get(f'https://sgp1.blynk.cloud/external/api/update?token=7ipFMtKuX0ghOOHdU0N_rBTY9vNzwq7o&V2={state}')
    return r.status_code


@app.get("/smart_predict/")
def predict_push_blynk(
        soil_moisture: int,
        air_temperature: float,
        air_humidity: float
):

    print(f"<> soil_moisture: {soil_moisture} <> temperature: {air_temperature} <> humidity: {air_humidity}")

    p = predict(soil_moisture, air_temperature, air_humidity)
    scode = 404
    if p <= 0.5:
        scode = set_blynk_pump_state(0)

    else:
        scode = set_blynk_pump_state(1)

    return {
        'code': scode,
        'probability': p
    }
