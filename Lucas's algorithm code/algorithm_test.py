  
from algo import Algorithm

if __name__ == "__main__":
    grid = [
        [1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1],
        [1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1],
        [1,1,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1],
        [1,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,1],
        [1,0,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1],
        [1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,1],
        [1,0,1,1,1,1,1,1,1,0,0,0,1,1,1,0,1,0,1],
        [1,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,1],
        [1,1,1,0,0,0,1,1,1,1,1,0,1,0,1,0,1,0,1],
        [1,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0,1],
        [1,0,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,0,1],
        [1,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,1],
        [1,1,1,1,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1],
        [0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1],
        [1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1]
    ]

    start = (13, 0)
    goal = (0, 17)
    explored_order = []
    algorithm = Algorithm(grid, start, goal, explored_order)
    algorithm.astar()
    path = algorithm.bfs()



    print('test bfs algorithm ----------------------------------------------')
    if path is None:
        print("No path found")
    else:
        print("Shortest path:")
        print(path)
        print("Number of moves:", len(path) - 1)

    print("Explored order:", explored_order)


    print('test astar algorithm ----------------------------------------------')
    if path is None:
        print("No path found")
    else:
        print("Shortest path:")
        print(path)
        print("Number of moves:", len(path) - 1)

    print("Explored order:", explored_order)