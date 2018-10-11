#include "CurlUtils.h"

size_t CurlUtils::writeCallback(void *contents, size_t size, size_t nmemb, void *userp) {
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

std::string CurlUtils::replaceAll(std::string str, const std::string& from, const std::string& to) {
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

nlohmann::json CurlUtils::runRequest(std::string request, std::string endpoint, std::map<std::string, std::string> options, std::string authorizationToken, std::string body) {
	CURL* curl;

	curl = curl_easy_init();
	if (!curl) {
		//	Throw error
		throw CurlException("Error: Could not initialize cURL");
		// return curl;
	}

	std::string url = "https://api.spotify.com" + endpoint;

	if (!options.empty()) {
		url += "?";
		for (auto option : options) {
			url += option.first + "=" + option.second + "&";
		}
	}
	replaceAll(url, " ", "%20");

	std::string writeBuffer;
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &writeBuffer);

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, request.c_str());

	if (!authorizationToken.empty()) {
		std::string header = "Authorization: Bearer " + authorizationToken;
		struct curl_slist* headers = NULL;
		headers = curl_slist_append(headers, header.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	}

	if (!body.empty()) {
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
	}

	int responseCode = curl_easy_perform(curl);
	if (responseCode != CURLE_OK) {
		//	Throw curl exception
		throw CurlException("Error: \n" + std::to_string(responseCode));
	}

	long statusCode = 0;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &statusCode);
	if (statusCode < 200 || statusCode > 204) {
		throw SpotifyException("Error: \n" + writeBuffer);
	}

	curl_easy_cleanup(curl);

	if (writeBuffer.empty()) {
		return nlohmann::json();
	}

	return nlohmann::json(writeBuffer);
}

nlohmann::json CurlUtils::GET(std::string endpoint, std::map<std::string, std::string> options, std::string authorizationToken, std::string body) {
	return runRequest("GET", endpoint, options, authorizationToken, body);
}

nlohmann::json CurlUtils::POST(std::string endpoint, std::map<std::string, std::string> options, std::string authorizationToken, std::string body) {
	return runRequest("POST", endpoint, options, authorizationToken, body);
}

nlohmann::json CurlUtils::PUT(std::string endpoint, std::map<std::string, std::string> options, std::string authorizationToken, std::string body) {
	return runRequest("PUT", endpoint, options, authorizationToken, body);
}

nlohmann::json CurlUtils::DELETE(std::string endpoint, std::map<std::string, std::string> options, std::string authorizationToken, std::string body) {
	return runRequest("DELETE", endpoint, options, authorizationToken, body);
}