import matplotlib.pyplot as plt

class dataDisplay:
    def __init__(self, title, fontSize=14, value=0):
        self.title   = title
        self.fontSize  = fontSize
        self.value = value
        self.outline = plt.Rectangle((0, 0), 1, 1, color='black')
        self.inner   = plt.Rectangle((0.02, 0.03), 0.96, 0.92, color='white')
        self.objects = [self.outline, self.inner]

    def setValue(self, value):
        self.value = value