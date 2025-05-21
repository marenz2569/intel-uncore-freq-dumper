# intel-uncore-freq-dumper

This tools dumps the intel uncore frequency for each socket.

## Usage
```
Usage:
  ./src/intel-uncore-freq-dumper [OPTION...]

      --measurement-duration arg
                                Duration of the uncore frequency 
                                measurement in milliseconds (default: 
                                10000)
      --measurement-interval arg
                                Interval of measurements in milliseconds 
                                (default: 10)
      --start-delta N           Cut of first N milliseconds of measurement 
                                (default: 5000)
      --stop-delta N            Cut of last N milliseconds of measurement 
                                (default: 2000)
      --outfile arg             The path where the results should be saved 
                                to. (default: outfile.csv)
```