#ifndef COMPARISONRUNNER_H
#define COMPARISONRUNNER_H

#include "ConnectionConfig.h"
#include "DatabaseDiffReport.h"

DatabaseDiffReport runDatabaseComparison(const ConnectionConfig& cfgA,
                                         const ConnectionConfig& cfgB);



#endif // COMPARISONRUNNER_H
