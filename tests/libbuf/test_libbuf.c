#include <errno.h>
#include <fcntl.h>
#include <paths.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <atf-c.h>

ATF_TC_WITHOUT_HEAD(test_test);
ATF_TC_BODY(test_test, tc)
{
}

ATF_TP_ADD_TCS(tp)
{

	ATF_TP_ADD_TC(tp, test_test);
	return (atf_no_error());
}
