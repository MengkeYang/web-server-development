import random
from locust import HttpUser, task, between

class QuickstartUser(HttpUser):
    wait_time = between(10, 15)

    @task
    def echoHandler(self):
        self.client.get('/echo')
        
    def on_start(self):
        pass


