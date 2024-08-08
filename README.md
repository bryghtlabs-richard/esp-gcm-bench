# Example: GCM multiplication benchmark

This test code runs several different `gcm_mult()` implementations and prints the cycles taken by each.

## How to use example

Before project configuration and build, be sure to set the correct chip target using `idf.py set-target <chip_name>`.
Then run `idf.py build flash monitor`

### Hardware Required

* A development board with any Espressif SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for Power supply and programming

## Example Output

As you run the example, you will see the following log:

```
...
I (318) main: Timing report:
        cMbedOld:1219(76.2c/B)
        cMbedSmall:4140(258.8c/B)
        cMbedLarge:2197(137.3c/B)
        cRichard:918(57.4c/B)
```

## Troubleshooting

For any technical queries, please [comment](https://github.com/espressif/esp-idf/pull/14314) on GitHub.