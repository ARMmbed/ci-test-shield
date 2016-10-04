The assumptions folder contains tests for assumptions made on the CI Test Shield. These tests are sanity checks and here to help you debug other failures. 

If any of these tests fail then the associated tests in the API folder will also fail, this is due to certain assumptions having to be made for the CI Test shield. 

If all of the assumptions tests pass then any failures in the API folder are entirely due to defects in the platform and should be debugged further. 