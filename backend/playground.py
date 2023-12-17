from backend.MyModel import MyModel
import datetime as dt

mymodel = MyModel('my_dataset.csv')

def predict(
        soil_moisture: int,
        air_temperature: float,
        air_humidity: float
):

    hourofday = dt.datetime.now().hour
    #
    probability = mymodel.predict(
        soil_moisture=soil_moisture,
        temperature=air_temperature,
        humidity=air_humidity,
        hour_ofday=hourofday
    )

    return {
        'hour_of_day': hourofday,
        'probability': probability[0]
    }


if __name__ == '__main__':
    p = predict(50, 50, 50)
    print(p)