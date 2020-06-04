import random
from locust import HttpUser, task, between

class QuickstartUser(HttpUser):
    wait_time = between(10, 15)

    @task
    def echoHandler(self):
        self.client.get('/echo')

    @task
    def staticHandlerCat(self):
        self.client.get('/static/mobydick.txt')

    @task
    def statusHandler(self):
        self.client.get('/status')
        
    def on_start(self):
        pass


