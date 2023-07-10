use std::env;
use std::fs::File;
use std::io::{BufRead, BufReader, Write};

struct Task {
    name: String,
    exec_time: i32,
}

fn task_staggering(task_path: &str, processors: i32) -> Vec<Vec<(String, i32, i32)>> {
    let mut task_list: Vec<Task> = Vec::new();

    let file = File::open(task_path).expect("error opening file");
    let reader = BufReader::new(file);
    
    for line in reader.lines() {
        if let Ok(line) = line {
            let new_line: Vec<&str> = line.trim().split(' ').collect();

            //println!("{:?}", new_line);

            let name = new_line[0].to_string();
            let exec_time = new_line[1].parse::<i32>().expect("error in parse");

            let task = Task { name, exec_time };
            task_list.push(task);
        }
    }

    task_list.sort_by_key(|time| time.exec_time);

    let mut processors_list: Vec<Vec<(String, i32, i32)>> = Vec::new();
    
    for _ in 0..processors {
        processors_list.push(Vec::new());
    }

                                            //?
    let mut process_time: Vec<i32> = vec![0; processors.try_into().unwrap()];

    for task in task_list {
        let processor_num = process_time.iter()
            .position(|&time| time == *process_time.iter().min().unwrap()).unwrap();

        let start: i32 = process_time[processor_num];
        let end: i32 = start + task.exec_time;

        process_time[processor_num] = end;

        processors_list[processor_num].push((task.name, start, end));
    }

    processors_list
}



fn main() {
    let args: Vec<String> = env::args().collect();

    let task_file = &args[1];
    let processors = args[2].parse::<i32>().expect("error in parse");

    // println!("{}", task_file);
    // println!("{}", processors);

    let process_list = task_staggering(task_file, processors);

    let mut file = File::create("resultado.txt").expect("error creating file");

    for (i, process) in process_list.iter().enumerate() {
        writeln!(&mut file, "Processador_{}", i + 1).expect("error to write on file");

        for task in process {
            writeln!(&mut file, "{};{};{}", task.0, task.1, task.2).expect("error to write on file");
        }

        writeln!(&mut file).expect("error to write on file");
    }
}


