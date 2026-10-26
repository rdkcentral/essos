# Essos L1 tests

Run the isolated unit suite from the repository root with `cmake -S L1 -B L1/build -DENABLE_COVERAGE=ON`, `cmake --build L1/build --parallel`, then `ctest --test-dir L1/build -L Essos --output-on-failure`. Generate the scoped report with `cmake --build L1/build --target coverage-essos`; it enforces a 75% line-coverage minimum for the explicitly selected `essos.cpp` API and event-dispatch functions.

The tests compile the real Essos implementation with `UNIT_TEST` backend seams and use local EGL stubs; they do not connect to a display server or enumerate input devices. Production builds do not define `UNIT_TEST`. The GitHub Actions workflow publishes the reports and test log as `artifacts-L1-essos`.
