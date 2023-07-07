import sys

class Task:
    def __init__(self, name, exec_time):
        self.name = name
        self.exec_time = exec_time

def task_staggering(task_file, processors):
    task_list = []
    
    with open(task_file, 'r') as file:
        for line in file:
            new_line = line.strip().split(" ")
            
            name = new_line[0]
            exec_time = new_line[1]
            
            task = Task(name, exec_time)
            task_list.append(task)
            
    task_list.sort(key=lambda time: int(time.exec_time))
    
    processors_list = []
    for i in range(processors):
        processors_list.append([])
        
    process_time = [0] * processors

    # for task in task_list:
    #     print(task.name, task.exec_time)

    for task in task_list:
        processor_num = process_time.index(min(process_time))
        
        start = process_time[processor_num]
        end = start + int(task.exec_time)
        
        process_time[processor_num] = end
        
        processors_list[processor_num].append((task.name, start, end))
        
    return processors_list    

def main():
    task_file = str(sys.argv[1])
    processors = int(sys.argv[2])
    
    processs_list = task_staggering(task_file, processors)
    
    with open('resultado.txt', 'w') as f:
        for i, process in enumerate(processs_list):
            f.write(f"Processador_{i+1}\n")
            
            for task in process:
                f.write(f'{task[0]};{task[1]};{task[2]}\n')
                
            f.write(f"\n")
            
if __name__ == '__main__':
    main()