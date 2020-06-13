from flask import Response, Flask, request
import prometheus_client
from prometheus_client.core import CollectorRegistry
from prometheus_client import Summary, Counter, Histogram, Gauge
import time
import random
import serial
# from serial import Serial

app = Flask(__name__)

_INF = float("inf")

# ser = serial.Serial('/dev/ttyS9', 57600)

graphs = {}
graphs['c'] = Counter('python_request_operations_total', 'The total number of processed requests')
graphs['h'] = Histogram('python_request_duration_seconds', 'Histogram for the duration in seconds.', buckets=(1, 2, 5, 6, 10, _INF))
graphs['g'] = Gauge('my_inprogress_requests', 'Description of gauge')

@app.route("/")
def hello():
    start = time.time()
    graphs['c'].inc()
    line = ""
        
    end = time.time()
    graphs['h'].observe(end - start)
    val =  (random.random() * 100) % 100
    graphs['g'].set(val)

    return "Hello World! {}".format(str(val))

def get_serial_data():
    val =  (random.random() * 100) % 100
    graphs['g'].set(val)
    return "Hello World!" + str(val)

@app.route("/metrics")
def requests_count():
    get_serial_data()
    res = []
    for k,v in graphs.items():
        res.append(prometheus_client.generate_latest(v))
    return Response(res, mimetype="text/plain")