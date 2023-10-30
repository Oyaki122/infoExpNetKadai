import heapq

def dijkstra(graph, start, end):
    """Function to find the shortest path using Dijkstra's algorithm."""
    shortest_path = {vertex: float('infinity') for vertex in graph}
    shortest_path[start] = 0
    priority_queue = [(0, start)]
    while priority_queue:
        current_distance, current_vertex = heapq.heappop(priority_queue)
        if current_distance > shortest_path[current_vertex]:
            continue
        for neighbor, weight in graph[current_vertex].items():
            distance = current_distance + weight
            if distance < shortest_path[neighbor]:
                shortest_path[neighbor] = distance
                heapq.heappush(priority_queue, (distance, neighbor))
    return shortest_path[end], shortest_path

def find_all_paths(graph, start, end, path=[]):
    """Function to find all paths from start to end in the graph."""
    path = path + [start]
    if start == end:
        return [path]
    if start not in graph:
        return []
    paths = []
    for node in graph[start]:
        if node not in path:
            new_paths = find_all_paths(graph, node, end, path)
            for new_path in new_paths:
                paths.append(new_path)
    return paths

if __name__ == "__main__":
    with open("checkers.log", "r") as f:
        log_data = f.read()

    node_data = log_data.split("-----------------------------------------")
    nodes = ["node1", "node2", "node3", "node4", "node5"]
    

    graph = {node: {} for node in nodes}
    for data in node_data:
        lines = data.strip().split("\n")
        if not lines or "No iperf3 processes were running" in data:
            continue
        current_node = None
        for line in lines:
            if "Server-side bandwidth measurements" in line:
                current_node = line.split(" ")[0].lstrip("[").rstrip("]")
            elif current_node and line.replace('.', '', 1).isdigit():
                bandwidth = float(line)
                for node in nodes:
                    if node != current_node and node not in graph[current_node]:
                        graph[current_node][node] = 1  / bandwidth
                        break
    all_paths = find_all_paths(graph, 'node1', 'node3')
    
    path_distances = []
    for path in all_paths:
        distance = 0
        for i in range(len(path) - 1):
            distance += graph[path[i]][path[i+1]]
        path_distances.append((path, distance))
    
    path_distances.sort(key=lambda x: x[1])
    
    # Print the top 5 paths
    for i, (path, distance) in enumerate(path_distances[:5]):
        print(f"{i+1}. Path: {' -> '.join(path)}, Expected Time: {distance:.2e} seconds")
