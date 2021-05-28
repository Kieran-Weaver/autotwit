# Autotwit

Data mining archive.org's twitter stream archives on consumer hardware using standard Linux tools, Python, and C++.

## Concept

The Internet Archive's [Twitter Stream](https://archive.org/search.php?query=collection%3Atwitterstream) is an archive of Twitter's "Spritzer" stream, which has 1% of the tweets on the platform. As of May 2021, the months from September 2011 to January 2021 are stored as a series of zip and tar archives filled with bzip2-compressed JSON files. Each json.bz2 file is a series of newline-delimited JSON documents, which follow the format seen in [Twitter's Documentation](https://developer.twitter.com/en/docs/twitter-api/v1/data-dictionary/overview).

These large tarballs and zip files are designed to be analyzed using Big Data tools such as Apache Hadoop, but they can just as easily be analyzed on consumer hardware. The tools in this repo are designed to scale down as far as laptops released in 2008 with a Core 2 Duo processor, 2 GB of RAM, and 120 GB of storage.

This repo contains the tools developed in the process of downloading and analyzing the Twitter data. The tools were initially developed locally, and were moved to GitHub when the scope of the project grew.

## Tools

### Downloading

`scripts/pool.py` automates downloading and extracting tarballs, one month at a time. The output is one .csv.zstd file for archived day of the month, with format

`User ID,Display Name,User Description,Tweet Content`

compressed using Zstandard. Strings are escaped using quotes, and \ is used to escape characters such as newline, tab, or " which would break CSV readers.

`tools/wayback.cpp` is used by `scripts/pool.py` to convert the JSON input to CSV format.

### CSV Filters

`tools/ascii` filters the CSV data to only include lines that contain all ASCII characters.

`tools/csvcut` selects columns from the CSV data, depending on which options are provided. For example, running `./tools/csvcut -3 -1 -2 -0` on a CSV file containing `"a","b","c","d"` will print out `"d","b","c","a"`.

### TF-IDF

`tools/idftool` processes CSV data to generate a list of unique words, and IDF table, and a XOR filter. By default it places them in the current working directory, but that can be changed using the `-p prefix` option.
