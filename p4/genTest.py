import os
import random


MAX_POINTS = 10            
MODES = ["MST", "FASTTSP", "OPTTSP"] 


COORD_MIN = -20
COORD_MAX = 20



def generate_random_points(n):
    pts = []
    for _ in range(n):
        x = random.randint(COORD_MIN, COORD_MAX)
        y = random.randint(COORD_MIN, COORD_MAX)
        pts.append((x, y))
    return pts

def write_test_file(index, mode, points):
    filename = f"test-{index}-{mode}.txt"


    with open(filename, "w") as f:
        f.write(str(len(points)) + "\n")
        for x, y in points:
            f.write(f"{x} {y}\n")

    print(f"Generated {filename}")




def generate_tests(num_tests=5):


    pts = generate_random_points(10)
    write_test_file(10, "FASTTSP", pts)



if __name__ == "__main__":

    generate_tests(num_tests=1)
