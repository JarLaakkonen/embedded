#include <gtest/gtest.h>
#include "../TimeParser.h"

TEST(TimeParserTest, TestCaseCorrectTime) {
    char time_test[] = "061205";
    ASSERT_EQ(time_parse(time_test),6*3600+12*60+5);
}
   TEST(TimeParserTest, TestCaseWrongTime) {
    char time_test[] = "141265";
    ASSERT_EQ(time_parse(time_test),TIME_VALUE_OVERFLOW_ERROR);
}
TEST(TimeParserTest, TestCaseNegativeTime) {
    char time_test[] = "-11205";
    ASSERT_EQ(time_parse(time_test),TIME_VALUE_NEGATIVE_ERROR);
}   
TEST(TimeParserTest, TestCaseSmallSizeTime) {
    char time_test[] = "205";
    ASSERT_EQ(time_parse(time_test),TIME_VALUE_SIZE_ERROR );
} 
TEST(TimeParserTest, TestCaseBigSizeTime) {
    char time_test[] = "12341205";
    ASSERT_EQ(time_parse(time_test),TIME_VALUE_SIZE_ERROR );
} 
TEST(TimeParserTest, TestCaseCharTime) {
    char time_test[] = "11a222";
    ASSERT_EQ(time_parse(time_test),TIME_VALUE_CHAR_ERROR );
}      
TEST(TimeParserTest, TestCaseNullTime) {
    char *time_test = NULL;
    ASSERT_EQ(time_parse(time_test),TIME_VALUE_NULL_ERROR );
} 
/*    
TEST(TimeParserTest, TestCaseZeroTime) {
    char *time_test = "000000";
    ASSERT_EQ(time_parse(time_test),TIME_VALUE_SECOND_ZERO_ERROR );
}    
*/
TEST(LightParseTest, TestCaseNullLight) {
    const char *light = NULL;
    ASSERT_EQ(light_parse(light),LIGHT_NULL_ERROR );
}
TEST(LightParseTest, TestCaseNumericLight) {
    const char *light = "GR123";
    ASSERT_EQ(light_parse(light),LIGHT_NUMERIC_ERROR );
}
TEST(LightParseTest, TestCaseLowerLight) {
    const  char *light = "GRgR";
    ASSERT_EQ(light_parse(light),LIGHT_LOWER_CASE_ERROR );
}
TEST(LightParseTest, TestCaseNotCharLight) {
    const char *light = "GR-RG";
    ASSERT_EQ(light_parse(light),LIGHT_NOT_CHAR_ERROR );
}
TEST(LightParseTest, TestCaseInvalidLight) {
    const char *light = "GRZR";
    ASSERT_EQ(light_parse(light),LIGHT_INVALID_VALUE_ERROR );
}
TEST(LightParseTest, TestCaseOkLight) {
    const char *light = "GRY";
    ASSERT_EQ(light_parse(light),LIGHT_VALUES_OK );
}
// https://google.github.io/googletest/reference/testing.html
// https://google.github.io/googletest/reference/assertions.html
