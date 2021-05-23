# Autotwit

Data mining archive.org's twitter stream archives on consumer hardware using standard Linux tools, Python, and C++.

## Concept

The Internet Archive's [Twitter Stream](https://archive.org/search.php?query=collection%3Atwitterstream) is an archive of Twitter's "Spritzer" stream, which has 1% of the tweets on the platform. As of May 2021, the months from September 2011 to January 2021 are stored as a series of zip and tar archives filled with bzip2-compressed JSON files. Each json.bz2 file is a series of newline-delimited JSON documents, which follow the format seen in [Twitter's Documentation](https://developer.twitter.com/en/docs/twitter-api/v1/data-dictionary/overview).

These large tarballs and zip files are designed to be analyzed using Big Data tools such as Apache Hadoop, but they can just as easily be analyzed on consumer hardware. The tools in this repo are designed to scale down as far as laptops released in 2008 with a Core 2 Duo processor, 2 GB of RAM, and 120 GB of storage.

This repo contains the tools developed in the process of downloading and analyzing the Twitter data.

## Tools
