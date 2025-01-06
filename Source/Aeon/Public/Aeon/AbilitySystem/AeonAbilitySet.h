/*
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include "GameplayTagContainer.h"
#include "AeonAbilitySet.generated.h"

struct FActiveGameplayEffectHandle;
struct FGameplayAbilitySpecHandle;
class UAbilitySystemComponent;
class UAeonGameplayAbility;
class UAttributeSet;
class UGameplayEffect;

/**
 * Data representing a Gameplay Ability that can be granted when granting an AbilitySet.
 */
USTRUCT(BlueprintType)
struct FAeonGameplayAbilityEntry
{
    GENERATED_BODY()

    /** The Gameplay Ability to grant. */
    UPROPERTY(EditDefaultsOnly, meta = (AllowAbstract = "false"))
    TSubclassOf<UAeonGameplayAbility> Ability{ nullptr };

    /** The Base Level of the ability to grant. This may be modified during when granted to an ASC. */
    UPROPERTY(EditDefaultsOnly)
    int32 Level{ 1 };

    /** Tag indicating input that can trigger the Ability. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "Input"))
    FGameplayTag InputTag{ FGameplayTag::EmptyTag };
};

/**
 * Data representing a Gameplay Effect that can be granted when granting an AbilitySet.
 */
USTRUCT(BlueprintType)
struct FAeonGameplayEffectEntry
{
    GENERATED_BODY()

    /** The Gameplay Effect to grant. */
    UPROPERTY(EditDefaultsOnly, meta = (AllowAbstract = "false"))
    TSubclassOf<UGameplayEffect> Effect{ nullptr };

    /** The Base Level of the effect to grant. This may be modified during when granted to an ASC. */
    UPROPERTY(EditDefaultsOnly)
    int32 Level{ 1 };
};

/**
 * Data representing an AttributeSet that can be granted when granting an AbilitySet.
 */
USTRUCT(BlueprintType)
struct FAeonAttributeSetEntry
{
    GENERATED_BODY()

    /** The AttributeSet to grant. */
    UPROPERTY(EditDefaultsOnly, meta = (AllowAbstract = "false"))
    TSubclassOf<UAttributeSet> AttributeSet{ nullptr };
};

class UAeonAbilitySet;

/**
 * A collection of handles and references as a result of granting a UAeonAbilitySet to an ASC.
 *
 * It should be noted that Gameplay Abilities and Gameplay Effects that were not successfully
 * granted will not appear in the lists below.
 */
USTRUCT(BlueprintType)
struct FAeonAbilitySetHandles
{
    GENERATED_BODY()

    friend UAeonAbilitySet;

private:
    /** The AbilitySystemComponent that the AeonAbilitySet was granted to. */
    UPROPERTY()
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent{ nullptr };

    /** The SpecHandle instances resulting from successfully granting Gameplay Abilities. */
    UPROPERTY()
    TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

    /** The EffectHandle instances resulting from successfully granting Gameplay Effects. */
    UPROPERTY()
    TArray<FActiveGameplayEffectHandle> EffectHandles;

    /** The references to "granted" AttributeSets. */
    UPROPERTY()
    TArray<TObjectPtr<UAttributeSet>> AttributeSets;

public:
    /**
     * Remove any granted abilities, effects and attributes from the associated AbilitySystemComponent.
     */
    void RemoveFromAbilitySystemComponent();

    /**
     * Return true if RemoveFromAbilitySystemComponent has not been invoked.
     *
     * @return true if the grants have not yet been removed.
     */
    bool IsValid() const;
};

/**
 * Container data asset that defines gameplay abilities, effects and attribute sets that can be granted to an ASC.
 *
 * NOTE: This is a renaming of the existing UAeonAbilitySystemGrantsBase and that is now considered deprecated. The
 * class was renamed and aligned with equivalent names that existed in the Lyra sample project to make it easier
 * to understand.
 */
UCLASS(BlueprintType, Const)
class UAeonAbilitySet final : public UPrimaryDataAsset
{
    GENERATED_BODY()

    /** Gameplay Abilities to grant to the ASC when the AeonAbilitySet is granted to the ASC. */
    UPROPERTY(EditDefaultsOnly, meta = (TitleProperty = Ability))
    TArray<FAeonGameplayAbilityEntry> Abilities;

    /** Gameplay Effects to grant to the ASC when the AeonAbilitySet is granted to the ASC. */
    UPROPERTY(EditDefaultsOnly, meta = (TitleProperty = Effect))
    TArray<FAeonGameplayEffectEntry> Effects;

    /** AttributeSets to grant to the ASC when the AeonAbilitySet is granted to the ASC. */
    UPROPERTY(EditDefaultsOnly, meta = (TitleProperty = AttributeSet))
    TArray<FAeonAttributeSetEntry> AttributeSets;

public:
    explicit UAeonAbilitySet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    /**
     * Grant the contained abilities, effects and attributes to the specified ASC.
     *
     * @param AbilitySystemComponent The ASC to grant the abilities, effects and attributes to.
     * @param OutGrantedHandles The handles that can be used to tak away anything that was granted. May pass nullptr.
     * @param LevelDelta The Delta to apply to Level of effects and abilities before granting them to the ASC.
     * @param SourceObject The source-object to apply when granting abilities.
     */
    void GiveToAbilitySystem(UAbilitySystemComponent* AbilitySystemComponent,
                             FAeonAbilitySetHandles* OutGrantedHandles,
                             int32 LevelDelta = 0,
                             UObject* SourceObject = nullptr) const;

#pragma region AActor Interface
#if WITH_EDITOR
    virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif // WITH_EDITOR
#pragma endregion
};
