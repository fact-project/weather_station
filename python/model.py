from peewee import MySQLDatabase, Model, IntegerField, FloatField, BooleanField, DateTimeField

db = MySQLDatabase("factdata")

class FactWeather(Model):
    cycles =  IntegerField()
    humidity0 = FloatField()
    humidity1 = FloatField()

    m152_counter = IntegerField()
    m152_state = BooleanField()
    rg1_drop_count = IntegerField()
    rg1_total_rainfall = FloatField()
    rg1_mean_drop_size = FloatField()
    rg2_tip_count = IntegerField()
    rg2_total_rainfall = FloatField()
    temperature0 = FloatField()
    temperature1 = FloatField()

    board_time = IntegerField()
    timestamp = DateTimeField()
    wind_direction = FloatField()
    wind_speed = FloatField()

    class Meta:
        database = db # This model uses the "people.db" database.
