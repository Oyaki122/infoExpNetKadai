import csv
import numpy as np
from ortools.graph.python import max_flow

class MaxFlow:
    def __init__(self):
        self.smf = max_flow.SimpleMaxFlow()
        self.start_nodes = np.array([
                                1,1,1,1,
                                2,2,2,2,
                                3,3,3,3,
                                4,4,4,4,
                                5,5,5,5
                                ])
        self.end_nodes = np.array([
                                2,3,4,5,
                                1,3,4,5,
                                1,2,4,5,
                                1,2,3,5,
                                1,2,3,4])
        self.capacities = np.array([
                                0.542,10.6,9.93,49.2,
                                2.08,9.79,9.85,0.542,
                                11.5,9.79,48.8,0.542,
                                9.94,2.08,48.8,49.1,
                                49.3,9.79,2.08,49.0])
    def forward(self,start:int,end:int):
        all_arcs = self.smf.add_arcs_with_capacity(self.start_nodes, self.end_nodes, self.capacities)
        status = self.smf.solve(start,end)
        solution_flows = self.smf.flows(all_arcs)
        data = []
        file_path = "./MaxFlow.csv"
        for arc, flow in zip(all_arcs, solution_flows):
            data.append([f"{self.smf.tail(arc)}",f"{self.smf.head(arc)}",f"{flow}"])
        header_row = ['start','end','flow']
        with open(file_path, "w", newline="") as f:
            writer = csv.writer(f)
            writer.writerow(header_row)
        with open(file_path, "a", newline="") as f:
            writer = csv.writer(f)
            writer.writerows(data)
        return 1

if __name__ == '__main__':
    maxflow = MaxFlow()
    maxflow.forward(3,1)