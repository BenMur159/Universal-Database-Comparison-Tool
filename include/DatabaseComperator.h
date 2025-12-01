#ifndef DATABASECOMPERATOR_H
#define DATABASECOMPERATOR_H

#include "IDatabaseConnection.h"
#include "DatabaseDiffReport.h"
#include "DatabaseSchemaInfoTypes.h"

DatabaseDiffReport compareDatabase(IDatabaseConnection& connA, IDatabaseConnection& connB,
                                   const DatabaseSchema& dbSchemaA, const DatabaseSchema& dbSchemaB);


#endif // DATABASECOMPERATOR_H
