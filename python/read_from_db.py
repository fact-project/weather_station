# coding: utf-8

import pandas as pd
from sqlalchemy import create_engine
from getpass import getpass
import matplotlib.pyplot as plt
db = create_engine('mysql+pymysql://fact:{}@fact-mysql.app.tu-dortmund.de/factdata'.format(getpass()))
df = pd.read_sql_table('weather', db)
df.head()
plt.figure()
df.plot('timestamp', 'temperature1')
df = pd.read_sql_table('weather', db)
