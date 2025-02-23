use std::env;
use std::fs::File;
use std::io::{self, Read, Result};

struct HexDumpConfig {
    cols: usize,
    groups: usize
}

trait FormatAsHex {
    fn format_as_hex(&self) -> String;
    fn format_as_hex_pretty_print(&self, hex_dump_config: HexDumpConfig) -> String;
}

impl FormatAsHex for Vec<u8> {
    fn format_as_hex(&self) -> String {
        self.iter().map(|byte| format!("{:02x}", byte)).collect()
    }

    fn format_as_hex_pretty_print(&self, hex_dump_config: HexDumpConfig) -> String {
        let mut hex_string = String::new();

        let max_in_one_line = hex_dump_config.cols * hex_dump_config.groups;
        for (i, byte) in self.iter().enumerate() {
            hex_string.push_str(&format!("{:02x}", byte));

            if (i + 1) % max_in_one_line == 0 {
                hex_string.push_str(&'\n'.to_string());
            }
            else if (i + 1) % hex_dump_config.groups == 0 {
                hex_string.push_str(&' '.to_string());
            }

        }
        hex_string
    }
}

fn read_from_stdin() -> Result<Vec<u8>> {
    let mut buf = Vec::new();
    io::stdin().read_to_end(&mut buf)?;
    Ok(buf)
}

fn read_from_file(path: &str) -> Result<Vec<u8>> {
    let mut buf = Vec::new();
    File::open(path)?.read_to_end(&mut buf)?;
    Ok(buf)
}

fn help() {
    println!("Bind\n");
    println!("Usage : <filename> or '-' for reading from stdin\n");
    println!("Options :");
    println!("  -c, --cols    format <cols> octets per line. [Default 8]");
    println!("  -g, --group   number of octets per group. [Default 2]");
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

fn parse_args() ->  Result<HexDumpConfig>  {
    let mut hex_dump_config = HexDumpConfig {
        cols: 16,
        groups: 2
    };

    let mut args = env::args().skip(1);
    while let Some(arg) = args.next() {
        match &arg[..] {
            "-h" | "--help" => help(),
            "-" | "--file" => {},
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
    let args: Vec<String> = env::args().collect();

    _ = parse_args();

    let buf;
    if args[1].chars().next() == Some('-') {
        buf = read_from_stdin()?;
    } else {
        buf = read_from_file(&args[1].as_str())?;
    }

    let hex_config = parse_args()?;

    let formated_buf = buf.format_as_hex_pretty_print(hex_config);

    println!("{formated_buf}");
    Ok(())
}
