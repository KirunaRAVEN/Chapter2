class indicatorLED:
    def __init__(self, text, pos):
        self.colors = [['dimgray', 'gray'], ['orangered', 'lightcoral']]
        self.outline = plt.Circle((0.5, 0.5), 0.49, color="black")
        self.outer = plt.Circle((0.5, 0.5), 0.48, color = colors[0][0])
        self.highlight = plt.Circle((0.5, 0.5), 0.44, color = colors[0][1])
        self.inner = plt.Circle((0.5, 0.5), 0.4, color = colors[0][0])
        self.text = text
        self.pos = pos
        self.state = 0
    
    def set_state(self, state):
        self.state = state
        self.outer.set_color(ledColors[state][0])
        self.highlight.set_color(ledColors[state][1])
        self.inner.set_color(ledColors[state][0])