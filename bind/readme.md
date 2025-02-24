# Bind

It's a binary dumper, think xdd, but with less features.

```
Usage : <filename> or '-' for reading from stdin

Options :
  -c, --cols               number of cols                      [Default 8]
  -g, --group              number of octets per group          [Default 2]
  -f, --file  <file_path>  file to read from ('-' for stdin)
  -r, --raw   <raw_style>  choose between hex, esc and C.
      hex : straight hex values ie 'cafe'
      esc : escaped hex values ie '\xca\xfe'
      C   : C include style ie '0xca, 0xfe'
```
