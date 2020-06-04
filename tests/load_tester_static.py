import random
from locust import HttpUser, task, between

class QuickstartUser(HttpUser):
    wait_time = between(10, 15)

    @task
    def staticHandlerCat(self):
        self.client.get('/static/mobydick.txt')
        
    def on_start(self):
        pass


