import ray.rllib.algorithms.ppo.ppo as ppo
import os

def main():
  found = False
  file_path = os.path.join(os.path.dirname(ppo.__file__), "ppo.py")
  contents = None
  with open(file_path, "r") as file:
    contents = file.readlines()
    for line in contents:
        if "default=0," in line:
            found = True

  if found:
    print("ppo.py was already fixed")
    exit()

  contents.insert(535, "                            ,default=0,\n")

  with open(file_path, "w") as file:
    file.writelines(contents)

  print(file_path + " patched, you can now run rlc")

if __name__ == "__main__":
    main()
