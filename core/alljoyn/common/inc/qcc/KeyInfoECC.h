# Contributed by Qualcomm Connected Experiences, Inc.,
# with authorization from the AllSeen Alliance, Inc.
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#     http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# 
# Pursuant to Section 1 of the License, the work of authorship constituting
# a Work and any Contribution incorporated in the Work shall mean only that
# Contributor's code submissions authored by that Contributor.  Any rights
# granted under the License are conditioned upon acceptance of these
# clarifications.
#ifndef _QCC_KEYINFO_ECC_H
#define _QCC_KEYINFO_ECC_H
/**
 * @file
 *
 * This file provide ECC public key info
 */

/******************************************************************************
 * Copyright (c) 2014, AllSeen Alliance. All rights reserved.
 *
 *    Contributed by Qualcomm Connected Experiences, Inc.,
 *    with authorization from the AllSeen Alliance, Inc.
 *    
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *    
 *        http://www.apache.org/licenses/LICENSE-2.0
 *    
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 *    
 *    Pursuant to Section 1 of the License, the work of authorship constituting
 *    a Work and any Contribution incorporated in the Work shall mean only that
 *    Contributor's code submissions authored by that Contributor.  Any rights
 *    granted under the License are conditioned upon acceptance of these
 *    clarifications.
 ******************************************************************************/

#include <qcc/platform.h>
#include <qcc/KeyInfo.h>
#include <qcc/CryptoECC.h>

namespace qcc {

/**
 * Abstract base class for various containers for cryptographic signatures.
 */
class SigInfo {

  public:

    /**
     * Used to specify the ECDSA SHA-256 Algorithm when specifying the
     * SigInfo algorithm.
     */
    static const size_t ALGORITHM_ECDSA_SHA_256 = 0;

    /**
     * Default constructor.
     */
    SigInfo(KeyInfo::FormatType format) : format(format), algorithm(0xFF)
    {
    }

    /**
     * desstructor.
     */
    virtual ~SigInfo()
    {
    }

    /**
     * Get the format
     * @return the format
     */
    const KeyInfo::FormatType GetFormat() const
    {
        return format;
    }

    /**
     * Retrieve the signature algorithm
     * @return the signature ECC algorithm
     */
    const uint8_t GetAlgorithm() const
    {
        return algorithm;
    }

    /**
     * Virtual initializer to be implemented by derived classes.  The derired
     * class should call the protected SigInfo::SetAlgorithm() method to set
     * the signature algorithm.
     */

    virtual void Init() = 0;

  protected:

    /**
     * Set the signature algorithm
     */
    void SetAlgorithm(uint8_t algorithm)
    {
        this->algorithm = algorithm;
    }


  private:
    /**
     * Assignment operator is private
     */
    SigInfo& operator=(const SigInfo& other);

    /**
     * Copy constructor is private
     */
    SigInfo(const SigInfo& other);

    KeyInfo::FormatType format;
    uint8_t algorithm;
};

/**
 * A container for an ECDSA signature
 */
class SigInfoECC : public SigInfo {

  public:

    /**
     * Default constructor.
     */
    SigInfoECC() : SigInfo(KeyInfo::FORMAT_ALLJOYN)
    {
        Init();
    }

    virtual void Init() {
        SetAlgorithm(ALGORITHM_ECDSA_SHA_256);
        memset(&sig, 0, sizeof(ECCSignature));
    }

    /**
     * destructor.
     */
    virtual ~SigInfoECC()
    {
    }

    /**
     * Assign the R coordinate
     * @param rCoord the R coordinate value to copy
     */
    void SetRCoord(const uint8_t* rCoord)
    {
        memcpy(sig.r, rCoord, ECC_COORDINATE_SZ);
    }
    /**
     * Retrieve the R coordinate value
     * @return the R coordinate value.  It's a pointer to an internal buffer. Its lifetime is the same as the object's lifetime.
     */
    const uint8_t* GetRCoord() const
    {
        return sig.r;
    }

    /**
     * Assign the S coordinate
     * @param sCoord the S coordinate value to copy
     */
    void SetSCoord(const uint8_t* sCoord)
    {
        memcpy(sig.s, sCoord, ECC_COORDINATE_SZ);
    }

    /**
     * Retrieve the S coordinate value
     * @return the S coordinate value.  It's a pointer to an internal buffer. Its lifetime is the same as the object's lifetime.
     */
    const uint8_t* GetSCoord() const
    {
        return sig.s;
    }

    /**
     * Set the signature.  The signature is copied into the internal buffer.
     * @param[in] sig the ECCSignature to copy into the internal buffer.
     */
    void SetSignature(const ECCSignature* sig)
    {
        memcpy(&this->sig, sig, sizeof(ECCSignature));
    }
    /**
     * Get the signature.
     * @return the signature.
     */
    const ECCSignature* GetSignature() const
    {
        return &sig;
    }

  private:
    /**
     * Assignment operator is private
     */
    SigInfoECC& operator=(const SigInfoECC& other);

    /**
     * Copy constructor is private
     */
    SigInfoECC(const SigInfoECC& other);

    ECCSignature sig;
};

/**
 * ECC KeyInfo
 */
class KeyInfoECC : public KeyInfo {

  public:

    /**
     * The ECC key type
     */
    static const size_t KEY_TYPE = 0;

    /**
     * The ECC algorithm
     */

    /**
     * Default constructor.
     */
    KeyInfoECC() : KeyInfo(FORMAT_ALLJOYN), curve(Crypto_ECC::ECC_NIST_P256)
    {
    }

    /**
     * constructor.
     */
    KeyInfoECC(uint8_t curve) : KeyInfo(FORMAT_ALLJOYN), curve(curve)
    {
    }

    /**
     * Default destructor.
     */
    virtual ~KeyInfoECC()
    {
    }

    /**
     * Retrieve the ECC algorithm
     * @return the ECC algorithm
     */
    const uint8_t GetAlgorithm() const
    {
        return SigInfo::ALGORITHM_ECDSA_SHA_256;
    }

    /**
     * Retrieve the ECC curve type.
     * @return the ECC curve type
     */
    const uint8_t GetCurve() const
    {
        return curve;
    }

    /**
     * Virtual member function that must be implemented so it returns
     * a pointer to the ECCPublicKey that was set.
     */
    virtual const ECCPublicKey* GetPublicKey() const
    {
        return NULL;
    }

    /**
     * Virtual member function that must be implemented so the public key is
     * assigned a value.
     *
     * @param[in] key public ECC key
     */
    virtual void SetPublicKey(const ECCPublicKey* key)
    {
    }

    /**
     * The required size of the exported byte array.
     * @return the required size of the exported byte array
     */
    const size_t GetExportSize() const;

    /**
     * Export the KeyInfo data to a byte array.
     * @param[in,out] buf the pointer to a byte array.  The caller must allocateenough memory based on call GetExportSize().
     * @return ER_OK for success; otherwise, an error code
     */

    QStatus Export(uint8_t* buf);

    /**
     * Import a byte array generated by a KeyInfo Export.
     * @param buf the export data
     * @param count the size of the export data
     * @return ER_OK for success; otherwise, an error code
     */

    QStatus Import(const uint8_t* buf, size_t count);

    /**
     * A String representation of the KeyInfoECC
     *
     * @param indent Number of space chars to indent the start of each line
     *
     * @return A String representation of the KeyInfoECC
     */
    virtual qcc::String ToString(size_t indent = 0) const;

    /**
     * Comparison operators equality
     * @param[in] other right hand side KeyInfoECC
     * @return true if the compared KeyInfoECC classes are equal.
     */
    bool operator==(const KeyInfoECC& other) const
    {
        if (curve != other.curve) {
            return false;
        }

        return KeyInfo::operator==(other);
    }

    /**
     * Comparison operators non-equality
     * @param[in] other right hand side KeyInfoECC
     * @return true if the two KeyInfoECC classes are not equal.
     */
    bool operator!=(const KeyInfoECC& other) const
    {
        return !(*this == other);
    }

  private:

    uint8_t curve;
};

/**
 * NIST P-256 ECC KeyInfo
 */
class KeyInfoNISTP256 : public KeyInfoECC {

  public:

    /**
     * Default constructor.
     */
    KeyInfoNISTP256() : KeyInfoECC(Crypto_ECC::ECC_NIST_P256)
    {
        /* using uncompressed */
        pubkey.form = 0x4;
    }

    /**
     * Copy constructor.
     */
    KeyInfoNISTP256(const KeyInfoNISTP256& other) : KeyInfoECC(Crypto_ECC::ECC_NIST_P256)
    {
        SetKeyId(other.GetKeyId(), other.GetKeyIdLen());
        SetPublicCtx(other.GetPublicCtx());
    }

    /**
     * Default destructor.
     */
    virtual ~KeyInfoNISTP256()
    {
    }

    /**
     * Get the public key context
     * @return the public key context
     */
    const uint8_t* GetPublicCtx() const
    {
        return (const uint8_t*) &pubkey;
    }

    /**
     * Get the ECCPublicKey from the KeyInfo
     * @return pointer to the KeyInfo ECCPublicKey
     */
    const ECCPublicKey* GetPublicKey() const
    {
        return &pubkey.key;
    }

    /**
     * Get the size of the public key in bytes.
     * @return the size of the public key in bytes.
     */
    size_t GetPublicSize() const
    {
        return sizeof (pubkey);
    }

    /**
     * Set the public key context
     * @param[in] ctx the public key context
     */
    void SetPublicCtx(const uint8_t* ctx)
    {
        memcpy((uint8_t*) &pubkey, ctx, sizeof (pubkey));
    }

    /**
     * Set the ECCPublicKey
     *
     * @param[in] key the ECCPublicKey
     */
    void SetPublicKey(const ECCPublicKey* key)
    {
        /* using uncompressed */
        pubkey.form = 0x4;
        memcpy((uint8_t*) &pubkey.key, (uint8_t*) key, sizeof (pubkey.key));
    }

    /**
     * The required size of the exported byte array.
     * @return the required size of the exported byte array
     */

    const size_t GetExportSize();

    /**
     * Export the KeyInfo data to a byte array.
     * @param[in,out] buf the pointer to a byte array.  The caller must allocateenough memory based on call GetExportSize().
     * @return ER_OK for success; otherwise, an error code
     */

    QStatus Export(uint8_t* buf);

    /**
     * Import a byte array generated by a KeyInfo Export.
     * @param buf the export data
     * @param count the size of the export data
     * @return ER_OK for success; otherwise, an error code
     */

    QStatus Import(const uint8_t* buf, size_t count);

    /**
     * A String representation of the KeyInfoNISTP256
     *
     * @param indent Number of space chars to indent the start of each line
     *
     * @return A String representation of the KeyInfoNISTP256
     */
    virtual qcc::String ToString(size_t indent = 0) const;

    /**
     * Comparison operators equality
     * @param[in] other right hand side KeyInfoNISTP256
     * @return true is keys are equal
     */
    bool operator==(const KeyInfoNISTP256& other) const
    {
        if (pubkey.form != other.pubkey.form) {
            return false;
        }

        if (pubkey.key != other.pubkey.key) {
            return false;
        }

        return KeyInfoECC::operator==(other);
    }

    /**
     * Comparison operators non-equality
     * @param[in] other right hand side KeyInfoNISTP256
     * @return true is keys are not equal
     */
    bool operator!=(const KeyInfoNISTP256& other) const
    {
        return !(*this == other);
    }

    /**
     * Assign operator for KeyInfoNISTP256
     *
     * @param[in] other the KeyInfoNISTP256 to assign
     *
     * @return the assigned KeyInfoNISTP256
     */

    KeyInfoNISTP256& operator=(const KeyInfoNISTP256& other)
    {
        if (this != &other) {
            SetKeyId(other.GetKeyId(), other.GetKeyIdLen());
            SetPublicCtx(other.GetPublicCtx());
        }
        return *this;
    }

  private:

    struct {
        uint8_t form;
        ECCPublicKey key;
    } pubkey;
};

} /* namespace qcc */


#endif
