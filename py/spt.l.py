class Node:
    def __init__(self, label, adjacent):
        self.vertex = label
        self.adjlist = adjacent

    def __str__(self):
        self.vertex.__str__ + self.adjlist.__str__


order = input("Enter the number of vertices: ")
i = 0
graph = []
labels = []
predecessors = []
while i < order:
    graph.append(Node(i, input().split())
