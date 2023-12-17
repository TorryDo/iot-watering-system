import pandas as pd
from sklearn.linear_model import LinearRegression
from sklearn.model_selection import train_test_split


class MyModel:

    dataset_fp = 'my_dataset.csv'

    c_soil_moisture = 'soil_moisture'
    c_air_tempe = 'air_temperature'
    c_air_humidity = 'air_humidity'
    c_hour = 'hour'
    c_state = 'state'

    lr: LinearRegression

    def __init__(self, dataset_fp: str):
        self.dataset_fp = dataset_fp

        df = pd.read_csv(dataset_fp)

        df = df[df[self.c_air_tempe].notna()]
        df = df[df[self.c_air_humidity].notna()]

        X = df.drop(columns=[self.c_state])
        y = df[self.c_state]

        X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=0)
        self.lr = LinearRegression()
        self.lr.fit(X_train, y_train)

    def predict(self, soil_moisture: int, temperature: float, humidity: float, hour_ofday: int) -> float:
        data = [[soil_moisture, temperature, humidity, hour_ofday]]
        input_data = pd.DataFrame(data,
                                  columns=[
                                      self.c_soil_moisture,
                                      self.c_air_tempe,
                                      self.c_air_humidity,
                                      self.c_hour
                                  ]
                                  )
        return self.lr.predict(input_data)
