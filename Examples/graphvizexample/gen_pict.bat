@ECHO OFF
dot -Tpng -o pre_schedule_test.png pre_schedule_test.dot  
dot -Tpng -o test.png test.dot

dot -Tsvg -o pre_schedule_test.svg pre_schedule_test.dot  
dot -Tsvg -o test.svg test.dot

dot -Tpdf -o pre_schedule_test.pdf pre_schedule_test.dot  
dot -Tpdf -o test.pdf test.dot