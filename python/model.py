from peewee import MySQLDatabase, Model, IntegerField, FloatField, BooleanField, DateTimeField

db = MySQLDatabase("factdata")

class FactWeather(Model):
    cycles =  IntegerField()
    humidity0 = FloatField(null = True)
    humidity1 = FloatField(null = True)

    m152_counter = IntegerField(null = True)
    m152_state = BooleanField(null = True)
    rg1_drop_count = IntegerField(null = True)
    rg1_total_rainfall = FloatField(null = True)
    rg1_mean_drop_size = FloatField(null = True)
    rg2_tip_count = IntegerField(null = True)
    rg2_total_rainfall = FloatField(null = True)
    temperature0 = FloatField(null = True)
    temperature1 = FloatField(null = True)

    board_time = IntegerField(null = True)
    timestamp = DateTimeField(unique = True, index = True)
    wind_direction = FloatField(null = True)
    wind_speed = FloatField(null = True)

    class Meta:
        database = db # This model uses the "people.db" database.
