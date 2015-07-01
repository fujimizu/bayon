_Tutorial of bayon in English_



# Introduction #

Bayon is a simple and fast [clustering](http://en.wikipedia.org/wiki/Cluster_analysis) tool for large-scale data sets. If you want to survey large-scale data, bayon is useful to partition the data into some groups and understand it.

Bayon supports two hard-clustering methods, repeated bisection clustering and [K-means clustering](http://en.wikipedia.org/wiki/K-means_clustering). In the outputs of these methods, each input document is assigned to only one cluster. But you can get similar clusters for each input document like soft-clustering method by using **--classifier** option.

# What's new #
  * 2012/06/18: 0.1.1 Released
    * A bug of build error was fixed

  * 2010/09/15: 0.1.0 Released
    * Refactoring

  * 2010/02/16: 0.0.10 Released
    * Bug fix

  * 2010/01/04: 0.0.9 Released
    * A bug that "make check" failed when prior version installed was fixed

  * 2009/12/24: 0.0.8 Released
    * Memory usage reduced when **-C, --classify** option specified

  * 2009/08/12: 0.0.7 Released
    * **--vector-size** option which is used to specify the max size of each input vector is added.
    * bug fix

  * 2009/07/17: 0.0.6 Released

# Download #

Please download [the latest version](http://code.google.com/p/bayon/downloads/list).

# Installation #

Download the latest source package, and type the following.

I recommend that you should install [google-sparsehash](http://code.google.com/p/google-sparsehash/) in advance to run bayon faster.

```
% tar xvzf bayon-*.tar.gz
% cd bayon-*
% ./configure
% make
% make check
% sudo make install
```

# Usage #

## Partition input data into several clusters ##

When you want to partition input data, run the command-line tool with the following options. The output of clustering will be printed to stdout.

The documents in each cluster are arranged in descending order of the similarity values which are the cosine similarities between the vectors of documents and the vector of the cluster centroid. Clustering with **-p, --point** option will print the similarity values the result of clustering.

```
% bayon -n num [options] file
% bayon -l limit [options] file
   -n, --number=num      number of clusters
   -l, --limit=lim       limit value of cluster bisection
   -p, --point           output similarity point
   -c, --clvector=file   save vectors of cluster centroids
   --clvector-size=num   max size of output vectors of
                         cluster centroids (default: 50)
   --method=method       clustering method(rb, kmeans), default:rb
   --seed=seed           set seed for random number generator
```

### Specify the number of output clusters ###

To specify the number of output clusters, perform clustering with **-n, --number** option.

```
(Partition input data into one hundred clusters)
% bayon -n 100 input.tsv > output.tsv

(Partition input data into one hundred clusters 
 and print the result with similarity values)
% bayon -n 100 -p input.tsv > output.tsv
```

### Specify the limit value of cluster partitions ###

When it is difficult to specify the number of output clusters preliminarily, perform clustering with **-l, --limit** option which specifies the limit value of cluster partitions. It's better to set the limit value to 1.0-2.0 to get the appropriate results of clustering.

```
(Set the limit value of cluster partition to 1.0)
% bayon -l 1.0 input.tsv > output.tsv
```

### Save the vectors of cluster centroids into a file ###

To save the vectors of cluster centroids, perform clustering with **-c, --clvector** option. The vectors will be saved into the specified file in text format.

Additionally, using **--clvector-size** option, you can specify the max number of the pairs of keys and points of centroid vectors. By default up to fifty pairs are saved.

```
(Save the vectors of cluster centroids into 'centroid.tsv')
% bayon -n 100 -c centroid.tsv input.tsv > output.tsv

(Up to twenty pairs are saved)
% bayon -n 100 -c centroid.tsv --clvector-size 20 input.tsv > output.tsv
```

If you want to get the similar clusters for each input document by using **-C, --classify** option, you need to save the vectors of cluster centroids.

### Change clustering methods ###

By default bayon uses 'Repeated Bisection Clustering' method. If you want to use 'K-means Clustering' method, perform clustering with **--method** option.

```
(Partition input data into a hundred clusters using K-means clustering method)
% bayon -n 100 --method kmeans input.tsv > output.tsv
```

Repeated bisection clustering is faster and more accurate than K-means clustering, so in general, it's better to use Repeated Bisection clustering.

### Change the seed value for random number generator ###

Bayon selects documents randomly during clustering, so you can get the different results of clustering by specifying a different seed value of random number generator. By default the same results of clustering are printed every time because the seed value is fixed.

```
(Set seed value to 123456)
% bayon -n 100 --seed 123456 input.tsv > output.tsv
```

## Specify the similar clusters for each input document ##

Performing the command-line tool with **-C, --classify** option, you can get the list of similar clusters for each input document. This option works like 'Soft Clustering'. The output will be printed to stdout.

```
% bayon -C file [options] file
   -C, --classify=file   target vectors
   --inv-keys=num        max size of keys of each vector to be
                         looked up in inverted index (default: 20)
   --inv-size=num        max size of inverted index of each key
                         (default: 100)
   --classify-size=num   max size of output similar groups
                         (default: 20)
```

When you use **-C, --classify** option, you need to perform clustering with **-c, --clvector** option preliminarily to save the vectors of cluster centroids

```
(At first, perform clustering input data saving the vectors of cluster centroids)
% bayon -c centroid.tsv -n 100 input.tsv > cluster.tsv

(Compare the vector of each document with the vector of each cluster centroid)
% bayon -C centroid.tsv input.tsv > classify.tsv
```

### Use options for inverted index ###

Specifying the similar clusters for each documents with **-C, --classify** option, bayon makes the [inverted indexes](http://en.wikipedia.org/wiki/Inverted_index) of the vectors of input clusters for speeding up. Bayon may work faster or more accurate by specifying the following options for inverted indexes.

  * **--inv-keys=num** : the max number of vector keys looked up inverted indexes
  * **--inv-size=num** : the max number of the related clusters for each vector key saved into inverted indexes

However, you don't need to specify above option usually.

## Common options ##

The following options can be used both clustering input data and specifying the similar clusters for each input document.

```
   --vector-size         max size of each input vector
   --idf                 apply idf to input vectors
   -h, --help            show help messages
   -v, --version         show the version and exit
```

Specifying **--vector-size** option, you can set the max number of the items in each input vector which may lead to run bayon faster and more accurate.

See next section for details about **--idf** option.

# Format of input data #

## List of input documents ##

The file of the list of input documents needs to be in a tab-separated text format as bellow. Each line must contain the information about only one document, the identifier of a document followed by the pairs of the keys and the values of document features. Blank lines must not be inserted.

```
document_id1 \t key1-1 \t value1-1 \t key1-2 \t value1-2 \t ...\n
document_id2 \t key2-1 \t value2-1 \t key2-2 \t value2-2 \t ...\n
...
```

  * _document\_id_: This is the identifier of a document, and must be the unique string which contains no tab characters.
  * _key_: This is the key of the feature of a document, and must be the string which contains no tab characters.
  * _value_: This is the value of the feature of a document, and must be real number.

_value_ needs to reflect the degree of the feature appropriately. So, you should apply weighting scheme (e.g. [tf-idf](http://en.wikipedia.org/wiki/TF_IDF)) to input data. Specifying **--idf** option, bayon changes the weights of input data automatically.

## List of the vectors of cluster centroids ##

The file of the vectors of cluster centroids needs to be in a tab-separated text format as below. This file is used as input data to get the similar clsuters for each documents with **-C, --classify** option. Each field (cluster\_id, key, value) is same as that of the list of input documents.

```
cluster_id1 \t key1-1 \t value1-1 \t key1-2 \t value1-2 \t ...\n
cluster_id2 \t key2-1 \t value2-1 \t key2-2 \t value2-2 \t ...\n
...
```

The file of the vectors of cluster centroids is saved by bayon with **-c, --clvector** option automatically, so you don't usually need to make it manually.

## Examples of input data ##

```
Alex   Pop      10    R&B      6    Rock     4
Bob    Jazz      8    Reggae   9
Dave   Classic   4    World    4
Ted    Jazz      9    Metal    2    Reggae   6
Fred   Pop       4    Rock     3    Hip-hop  3
Sam    Classic   8    Rock     1
```

In the above example, 'Alex', 'Bob', 'Ted' are the identifiers of the documents, and 'Alex' has the features as the genres of music (Pop, R&B, Rock) which he often listens.

# Format of output data #

## List of clusters ##

The file of the list of output clusters is in a tab-separated text format as below. Each line contains the information about one cluster, the identifier of the cluster followed by the identifiers of the documents in the cluster.

```
cluster_id1 \t document_id1 \t document_id2 \t document_id3 \t ...\n
cluster_id2 \t document_id4 \t document_id5 \t document_id6 \t ...\n
...
```

## List of clusters with similarity values ##

Clustering with **-p, --point** option, the similarity values are printed in the result of clustering. The similarity values are calculated by cosine similarity measure between the vectors of the documents and the vectors of the cluster centroids.

```
cluster_id1 \t key1-1 \t value1-1 \t key1-2 \t value1-2 \t ...\n
cluster_id2 \t key2-1 \t value2-1 \t key2-2 \t value2-2 \t ...\n
...
```

## List of the vectors of cluster centroids ##

Clustering with **-c, --clvector** option, the list of the vectors of cluster centroids are saved into the specified file. The format of output file is tab-separated text as below. Each line contains the information about one cluster, the identifier of a cluster followed by the pairs of the keys and the values of cluster centroid vectors.

```
cluster_id1 \t key1-1 \t value1-1 \t key1-2 \t value1-2 \t ...\n
cluster_id2 \t key2-1 \t value2-1 \t key2-2 \t value2-2 \t ...\n
...
```

## List of the similar clusters for each document ##

The list of similar clusters for each document which specified by using **-C, --classify** option is in a tab-separated text format as below. Each line contains the information about one document, the identifier of a document followed by the pairs of the identifiers of clusters and the similarity values.

```
document_id1 \t cluster_id1 \t point1 \t cluster_id2 \t point2 \t ...\n
document_id2 \t cluster_id3 \t point3 \t cluster_id4 \t point4 \t ...\n
...
```

## Examples of output data ##

  * The result of clustering the above example
```
1       Ted     Bob
2       Alex    Fred
3       Sam     Dave
```

  * The result of clustering the above example with **-p, --point** option
```
1       Ted     0.987737        Bob     0.987737
2       Alex    0.928262        Fred    0.928262
3       Sam     0.922401        Dave    0.922401
```

  * The vectors of the cluster centroids
```
1   Jazz    0.750476    Reggae  0.654458    Metal   0.0920378
2   Pop     0.806401    Rock    0.451887    Hip-hop 0.277129    R&B 0.262137
3   Classic 0.921175    World   0.383297    Rock    0.0672347
```

  * The result specified the similar clusters for each document
```
Alex    2       0.928261        3       0.0218138
Bob     1       0.987737
Dave    3       0.922401
Ted     1       0.987737
Fred    2       0.928262        3       0.034592
Sam     3       0.922401        2       0.0560497
```

# Performance #

The execution times to perform clustering and get the similar clusters (classification) are as bellow. Bayon could perform both clustering and classification fast.

| **# of data** | **# of cluster** | **time of clustering(sec)** | **time of classification(sec)** |
|:--------------|:-----------------|:----------------------------|:--------------------------------|
| 1000          | 10               | 0.17                        | 0.088                           |
| 50000         | 1000             | 36                          | 13.4                            |
| 500000        | 10000            | 720                         | 1385                            |