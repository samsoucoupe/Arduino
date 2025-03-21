const fs = require("fs");
const https = require("https");
const http = require("http");

/**
 * Vérifie si le schéma JSON contient des clés dupliquées au même niveau.
 */
function hasDuplicateKeys(schema, path = "", seenKeys = new Map()) {
    let errors = [];

    if (typeof schema !== "object" || schema === null) {
        return errors;
    }

    const keysAtThisLevel = new Set();

    for (let key in schema) {
        let fullPath = path ? `${path}.${key}` : key;

        // Vérifier si la clé a déjà été vue au même niveau
        if (keysAtThisLevel.has(key)) {
            errors.push(`🚨 Duplication de clé détectée : '${fullPath}'`);
        } else {
            keysAtThisLevel.add(key);
        }

        // Vérifier la récursion pour les objets imbriqués
        if (typeof schema[key] === "object" && !Array.isArray(schema[key])) {
            errors = errors.concat(
                hasDuplicateKeys(schema[key], fullPath, seenKeys),
            );
        }
    }

    return errors;
}

/**
 * Valide les valeurs de certains champs spécifiques dans `status` et `regul`.
 */
function validateSpecialValues(obj) {
    const errors = [];
    const validColdValues = ["ON", "OFF"];
    const validHeatValues = ["ON", "OFF"];
    const validRegulValues = ["COOL", "HALT", "HEAT"];

    // Validation des champs 'status.cold', 'status.heat', et 'status.regul'
    if (obj["status"]) {
        // Vérification de la valeur de 'cold' dans 'status'
        if (
            obj["status"]["cold"] &&
            !validColdValues.includes(obj["status"]["cold"])
        ) {
            errors.push(
                `🚨 Valeur invalide pour 'cold' : '${obj["status"]["cold"]}'`,
            );
        }

        // Vérification de la valeur de 'heat' dans 'status'
        if (
            obj["status"]["heat"] &&
            !validHeatValues.includes(obj["status"]["heat"])
        ) {
            errors.push(
                `🚨 Valeur invalide pour 'heat' : '${obj["status"]["heat"]}'`,
            );
        }

        // Vérification de la valeur de 'regul' dans 'status'
        if (
            obj["status"]["regul"] &&
            !validRegulValues.includes(obj["status"]["regul"])
        ) {
            errors.push(
                `🚨 Valeur invalide pour 'regul' : '${obj["status"]["regul"]}'`,
            );
        }
    }

    // Validation de l'objet 'regul' (en dehors de 'status')
    if (obj["regul"]) {
        // Vérification que 'lt' soit inférieur à 'ht'
        if (obj["regul"]["lt"] > obj["regul"]["ht"]) {
            errors.push(
                `🚨 Valeur invalide pour 'lt' et 'ht' : '${obj["regul"]["lt"]}' et '${obj["regul"]["ht"]}'`,
            );
        }
    }

    return errors;
}

/**
 * Valide si un JSON respecte le schéma défini.
 */
function validateESP32JSON(jsonString) {
    try {
        const data = JSON.parse(jsonString);

        const schema = {
            status: {
                temperature: "number",
                light: "number",
                regul: "string",
                fire: "boolean",
                heat: "string",
                cold: "string",
                fanspeed: "number",
            },
            location: {
                room: "string",
                gps: {
                    lat: "number",
                    lon: "number",
                },
                address: "string",
            },
            regul: {
                lt: "number",
                ht: "number",
            },
            info: {
                ident: "string",
                user: "string",
                loc: "string",
            },
            net: {
                uptime: "string",
                ssid: "string",
                mac: "string",
                ip: "string",
            },
            reporthost: {
                target_ip: "string",
                target_port: "number",
                sp: "number",
            },
        };

        // Vérifier les clés dupliquées dans le schéma
        let duplicateErrors = hasDuplicateKeys(schema);
        if (duplicateErrors.length > 0) {
            console.error(duplicateErrors.join("\n"));
            return false;
        }

        function validateObject(obj, schema, path = "") {
            if (typeof obj !== "object" || obj === null) {
                console.error(`❌ Erreur : '${path}' doit être un objet.`);
                return false;
            }

            const objKeys = Object.keys(obj);
            const schemaKeys = Object.keys(schema);

            // Détection des clés manquantes
            for (const key of schemaKeys) {
                if (!objKeys.includes(key)) {
                    console.error(
                        `❌ Champ manquant : '${path ? `${path}.` : ""}${key}'`,
                    );
                    return false;
                }
            }

            // Détection des clés en trop (tentative de hack)
            for (const key of objKeys) {
                if (!schemaKeys.includes(key)) {
                    console.error(
                        `🚨 Champ en trop détecté : '${path ? `${path}.` : ""}${key}'`,
                    );
                    return false;
                }
            }

            // Validation des valeurs spéciales (status.heat, status.cold, status.regul, regul)
            let specialErrors = validateSpecialValues(obj);
            if (specialErrors.length > 0) {
                console.error(specialErrors.join("\n"));
                return false;
            }

            // Validation des types
            return schemaKeys.every((key) => {
                const newPath = path ? `${path}.${key}` : key;

                if (
                    typeof schema[key] === "object" &&
                    !Array.isArray(schema[key])
                ) {
                    return validateObject(obj[key], schema[key], newPath);
                }

                if (typeof obj[key] !== schema[key]) {
                    console.error(
                        `❌ Type incorrect pour '${newPath}', attendu: ${schema[key]}, reçu: ${typeof obj[key]}`,
                    );
                    return false;
                }

                // Vérification des valeurs nulles
                if (obj[key] === null) {
                    console.error(`⚠️ Valeur nulle détectée : '${newPath}'`);
                }

                return true;
            });
        }

        return validateObject(data, schema);
    } catch (error) {
        console.error("❌ Erreur de syntaxe dans le JSON :", error.message);
        console.error("❌ Erreur : JSON invalide ou mal formé.");
        return false;
    }
}

/**
 * Charge un JSON depuis un fichier ou une URL et le valide.
 */
function fetchAndValidateJSON(source) {
    const processJSON = (data) => {
        if (validateESP32JSON(data)) {
            console.log("✅ Le JSON est valide.");
        } else {
            console.error("❌ Le JSON est invalide.");
            process.exit(1);
        }
    };

    if (source.startsWith("http://") || source.startsWith("https://")) {
        const client = source.startsWith("https://") ? https : http;
        client
            .get(source, (res) => {
                let data = "";
                res.on("data", (chunk) => (data += chunk));
                res.on("end", () => processJSON(data));
            })
            .on("error", (err) => {
                console.error(
                    "❌ Erreur : Impossible de récupérer le fichier JSON.",
                    err.message,
                );
                process.exit(1);
            });
    } else {
        fs.readFile(source, "utf8", (err, data) => {
            if (err) {
                console.error(
                    "❌ Erreur : Impossible de lire le fichier.",
                    err.message,
                );
                process.exit(1);
            }
            processJSON(data);
        });
    }
}

// Lecture du fichier ou de l'URL JSON
let source = process.argv[2] || "test.json";
fetchAndValidateJSON(source);
