#ifndef COMPARISONRUNNER_H
#define COMPARISONRUNNER_H

#include "dbdiff/ConnectionConfig.h"
#include "dbdiff/DatabaseDiffReport.h"

void registerAllBuiltInBackends();

DatabaseDiffReport runDatabaseComparison(const ConnectionConfig& cfgA,
                                         const ConnectionConfig& cfgB);



#endif // COMPARISONRUNNER_H
