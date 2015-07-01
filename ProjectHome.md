[Tutorial in Japanese](Tutorial_ja.md)

[Tutorial in English](Tutorial_en.md)

## Overview ##
**Bayon** is a simple and fast hard-clustering tool.

**Bayon** supports Repeated Bisection clustering and K-means clustering.

## Install ##
```
% ./configure
% make
% sudo make install
```

## Usage ##

### Clustering input data ###
```
% bayon -n num [options] file
% bayon -l limit [options] file
   -n, --number=num      the number of clusters
   -l, --limit=lim       limit value of cluster bisection
   -p, --point           output similarity points
   -c, --clvector=file   save the vectors of cluster centroids
   --clvector-size=num   max size of output vectors of
                         cluster centroids (default: 50)
   --method=method       clustering method(rb, kmeans), default:rb
   --seed=seed           set a seed for random number generator
```

### Get similar clusters for each input documents ###
```
% bayon -C file [options] file
   -C, --classify=file   target vectors
   --inv-keys=num        max size of the keys of each vector to be
                         looked up in inverted index (default: 20)
   --inv-size=num        max size of the inverted index of each key
                         (default: 100)
   --classify-size=num   max size of output similar groups
                         (default: 20)
```

### Common options ###
```
   --vector-size=num     max size of each input vector
   --idf                 apply idf to input vectors
   -h, --help            show help messages
   -v, --version         show the version and exit
```

## Example ##
  * clustering (number\_of\_output\_clusters = 100)
```
% bayon -n 100 input.tsv > cluster.tsv
```

  * clustering (save vectors of cluster centroids)
```
% bayon -n 100 -c centroid.tsv input.tsv > cluster.tsv
```

  * classification (get similar clusters for input documents)
```
% bayon -C centroid.tsv input.tsv > classify.tsv
```

## Format of Input Data ##

### List of the vectors of input documents for clustering and classification ###

```
document_id1 \t key1-1 \t value1-1 \t key1-2 \t value1-2 \t ...\n
document_id2 \t key2-1 \t value2-1 \t key2-2 \t value2-2 \t ...\n
...
```
  * document\_id : string
  * key         : string
  * value       : double

### List of the vectors of cluster centroids ###

```
cluster_id1 \t key1-1 \t value1-1 \t key1-2 \t value1-2 \t ...\n
cluster_id2 \t key2-1 \t value2-1 \t key2-2 \t value2-2 \t ...\n
...
```
  * cluster\_id : string
  * key      : string
  * value    : double

## Format of Output Data ##

### List of clusters (output of clustering) ###
```
cluster_id1 \t document_id1 \t document_id2 \t document_id3 \t ...\n
cluster_id2 \t document_id4 \t document_id5 \t document_id6 \t ...\n
...
```
  * cluster\_id  : integer (>= 1)
  * document\_id : string

### List of the clusters with similarity values between documents and clusters (if perform clustering with --point option) ###

```
cluster_id1 \t document_id1 \t point1 \t document_id2 \t point2 \t ...\n
cluster_id2 \t document_id3 \t point3 \t document_id4 \t point4 \t ...\n
...
```
  * cluster\_id  : integer (>= 1)
  * document\_id : string
  * point       : double

### List of the vectors of cluster centroids (if perform clustering with --clvector option) ###
```
cluster_id1 \t key1-1 \t value1-1 \t key1-2 \t value1-2 \t ...\n
cluster_id2 \t key2-1 \t value2-1 \t key2-2 \t value2-2 \t ...\n
...
```
  * cluster\_id  : integer (>= 1)
  * key        : string
  * value      : double

### List of similar clusters for each input documents ###
```
document_id1 \t cluster_id1 \t point1 \t cluster_id2 \t point2 \t ...\n
document_id2 \t cluster_id3 \t point3 \t cluster_id4 \t point4 \t ...\n
...
```
  * document\_id : string
  * cluster\_id    : string
  * point       : double

## Requirement ##
  * C++ compiler with STL (Standard Template Library)

### Recommended ###
  * [google-sparsehash](http://code.google.com/p/google-sparsehash/)
    * If google-sparsehash not installed, this clustering tool uses "gnu\_cxx::hash\_map" or "std::map"

## License ##
GPL2 (Gnu General Public License Version 2)

## Author ##
Mizuki Fujisawa <[fujisawa@bayon.cc](mailto:fujisawa@bayon.cc)>