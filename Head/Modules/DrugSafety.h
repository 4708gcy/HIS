#ifndef DRUGSAFETY_H
#define DRUGSAFETY_H

#include <string>
#include <utility>
#include <vector>

class DrugSafety {
public:
  static std::vector<std::pair<std::string, std::string>>
  loadInteractions(const std::string &filePath);
  static bool hasInteraction(
      const std::string &drug1, const std::string &drug2,
      const std::vector<std::pair<std::string, std::string>> &interactions);
  static std::vector<std::string>
  checkAllergyConflict(const std::string &allergyHistory,
                       const std::vector<std::string> &prescribedDrugNames);
  static std::vector<std::string> checkDDI(
      const std::string &newDrug, const std::vector<std::string> &existingDrugs,
      const std::vector<std::pair<std::string, std::string>> &interactions);
};

#endif // DRUGSAFETY_H
