
#include "FaceJSONDB.hpp"


CFaceJSONDB::CFaceJSONDB()
{
	m_nItemCount = 0;
	m_bInit = false;
}

int				//. Return 
CFaceJSONDB::LoadJSONFile(std::string p_strJSONPath)
{
	int			w_nSts = LD_JS_ERR_UNKNOWN;

	m_nItemCount = 0;
	m_bInit = false;

	std::ifstream w_JS_file(p_strJSONPath);
	if (w_JS_file.is_open() == false) {
		return LD_JS_ERR_PATH;
	}

	std::string jsonData((std::istreambuf_iterator<char>(w_JS_file)), std::istreambuf_iterator<char>());

	rapidjson::Document document;
	document.Parse(jsonData.c_str());

	m_faceDataDBList.clear();
	if (!document.HasParseError() && document.IsArray()) {

		for (const auto& face : document.GetArray()) {
			FaceData faceData;
			faceData.faceId = face["faceId"].GetString();
			faceData.features_version = face["features_version"].GetInt();
			faceData.features_flags = face["features_flags"].GetInt();
			faceData.features_type = face["features_type"].GetInt();

			const rapidjson::Value& features = face["features"];
			if (features.IsArray()) {
				int i = 0;
				for (const auto& f : features.GetArray()) {
					faceData.features[i] = (short)f.GetInt();
					i++;
				}
				m_nItemCount++;
			}

			m_faceDataDBList.push_back(faceData);
		}
	}


	//.OK
	m_bInit = true;
	w_nSts = LD_JS_SUCCESS;
	return w_nSts;
}

int				//. Return 
CFaceJSONDB::GetFaceData(int p_nPos, struct FaceData& p_stFaceData)
{
	int			w_nSts = LD_JS_ERR_UNKNOWN;

	if (m_bInit == false) {
		return LD_JS_ERR_INIT;
	}
	if (p_nPos < 0 || p_nPos > m_nItemCount - 1) {
		return LD_JS_ERR_PARAM;
	}

	p_stFaceData = m_faceDataDBList.at(p_nPos);

	//.OK
	w_nSts = LD_JS_SUCCESS;
	return w_nSts;
}

std::string		//. Face ID
CFaceJSONDB::GetFaceIDInfo(int p_nPos)
{
	std::string			w_FaceID = "";
	struct FaceData		w_FaceData;

	if (m_bInit == false) {
		return w_FaceID;
	}
	if (p_nPos < 0 || p_nPos > m_nItemCount - 1) {
		return w_FaceID;
	}

	w_FaceData = m_faceDataDBList.at(p_nPos);

	w_FaceID = w_FaceData.faceId;

	return w_FaceID;
}


int				//. Return 
CFaceJSONDB::GetDataCount(int& p_nCount)
{
	int			w_nSts = LD_JS_ERR_UNKNOWN;

	if (m_bInit == false) {
		return LD_JS_ERR_INIT;
	}

	p_nCount = m_nItemCount;

	//.OK
	w_nSts = LD_JS_SUCCESS;
	return w_nSts;

}

int				//. Return 
CFaceJSONDB::AddFaceData(struct FaceData p_stFaceData)
{
	int			w_nSts = LD_JS_ERR_UNKNOWN;

	if (m_bInit == false) {
		return LD_JS_ERR_INIT;
	}

	//. TODO

	//.OK
	w_nSts = LD_JS_SUCCESS;
	return w_nSts;

}
