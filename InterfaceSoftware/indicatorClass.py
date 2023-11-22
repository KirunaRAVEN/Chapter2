import matplotlib.pyplot as plt

class indicatorLight:
    def __init__(self, title, state=0, fontSize=20):
        self.title     = title
        self.state     = state
        self.fontSize  = fontSize
        self.color     = [['dimgray', 'gray'], ['orangered', 'lightcoral']]
        self.outline   = plt.Circle((0.5, 0.5), 0.49, color='black')
        self.outer     = plt.Circle((0.5, 0.5), 0.48, color=self.color[state][0])
        self.highlight = plt.Circle((0.5, 0.5), 0.44, color=self.color[state][1])
        self.inner     = plt.Circle((0.5, 0.5), 0.40, color=self.color[state][0])
        self.objects   = [self.outline, self.outer, self.highlight, self.inner]


    def setState(self, state):
        self.state = state
        self.outer.set_color(self.color[state][0])
        self.highlight.set_color(self.color[state][1])
        self.inner.set_color(self.color[state][0])