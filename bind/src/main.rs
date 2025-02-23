use std::env;
use std::fs::File;
use std::io::{self, Read, Result};
// use std::str::from_utf8;
use std::process;

struct BindConfig {
    cols: usize,
    groups: usize,
    file_name: String,
}

trait FormatAsHex {
    #[allow(dead_code)]
    fn format_as_hex(&self) -> String;
    fn format_as_hex_pretty_print(&self, bind_config: BindConfig) -> String;
}

impl FormatAsHex for Vec<u8> {
    fn format_as_hex(&self) -> String {
        self.iter().map(|byte| format!("{:02x}", byte)).collect()
    }

    fn format_as_hex_pretty_print(&self, bind_config: BindConfig) -> String {
        let mut hex_string = String::new();

        let max_in_one_line = bind_config.cols * bind_config.groups;
        let max_numer_of_lines = self.len() / max_in_one_line;
        // let mut bytes_written_in_line = 0;

        let mut line_num = 0;
        hex_string.push_str(&format!("{:08x}: ", line_num * 0x10)); // address prepend
        for (i, byte) in self.iter().enumerate() {
            hex_string.push_str(&format!("{:02x}", byte));
            // bytes_written_in_line += 1;

            if (i + 1) % max_in_one_line == 0 && line_num  < max_numer_of_lines {
                // hex_string.push_str(&format!("  {}", from_utf8(&self[i+1-bytes_written_in_line..=i]).unwrap())); // ascii
                hex_string.push_str(&'\n'.to_string());
                // bytes_written_in_line = 0;
                line_num += 1;
                hex_string.push_str(&format!("{:08x}: ", i + 1)); // address prepend
            }
            else if (i + 1) % bind_config.groups == 0 {
                hex_string.push_str(&' '.to_string());
            }
        }
        // hex_string.push_str(&format!(" {}", from_utf8(&self[self.len()-bytes_written_in_line..]).unwrap())); // ascii
        hex_string
    }
}

fn read_bytes(path: &String) -> Result<Vec<u8>> {
    let mut buf = Vec::new();
    if *path == '-'.to_string() {
        io::stdin().read_to_end(&mut buf)?;
    } else {
        File::open(path)?.read_to_end(&mut buf)?;
    }
    Ok(buf)
}

fn help() {
    println!("Bind\n");
    println!("Usage : <filename> or '-' for reading from stdin\n");
    println!("Options :");
    println!("  -c, --cols    number of cols. [Default 8]");
    println!("  -g, --group   number of octets per group. [Default 2]");
    println!("  -f, --file    file to read from ('-' for stdin)");
    // println!("  -l, --len     stop after <len> octets");
    // println!("  -a            prepend byte");
    process::exit(0);
}


fn parse_int(arg: &str, next_arg : Option<String>) -> usize {
    if let Some(some_arg) = next_arg {
        match some_arg.parse::<usize>() {
            Ok(o) => o,
            Err(e) => panic!("{e} : '{some_arg}'"),
        }
    } else {
        panic!("'{arg}' expects an argument none was given, use -h to see help");
    }
}

fn parse_args() ->  Result<BindConfig>  {
    let mut hex_dump_config = BindConfig {
        cols: 8,
        groups: 2,
        file_name: "-".to_string(),
    };

    let mut args = env::args().skip(1);
    while let Some(arg) = args.next() {
        match &arg[..] {
            "-h" | "--help" => help(),
            "-f" | "--file" => {
                if let Some(some_arg) = args.next() {
                    if some_arg.len() != 1 || some_arg.chars().next() != Some('-') {
                        hex_dump_config.file_name = some_arg;
                    }
                } else {
                    panic!("'{arg}' expects an argument none was given, use -h to see help");
                }
            },
            "-c" | "--cols" => hex_dump_config.cols = parse_int(&arg, args.next()),
            "-g" | "--groups" => hex_dump_config.groups = parse_int(&arg, args.next()),
            _ => {
                panic!("'{arg}' is not a valid argument, use -h to see help");
            }
        }
    }
    Ok(hex_dump_config)
}

fn main() -> Result<()> {
    let bind_config = parse_args()?;
    let buf = read_bytes(&bind_config.file_name)?;
    let formated_buf = buf.format_as_hex_pretty_print(bind_config);

    println!("{formated_buf}");
    Ok(())
}
